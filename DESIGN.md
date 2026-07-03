# Design Decisions

- Focus is on maintaining C++98 compatibility to learn more about the idioms and constraints of the timeframe. May clone this project in the future and modernize the C++98 codebase to C++23 or C++26.
- Single-process today until request/response flow can be fully demonstrated; will change to `fork` later and as we approach the C10k challenge, then we can switch to `epoll` or some other similar method of concurrency.

## Request parsing and error handling

- Parse errors are reported through a state field embedded in `Request` (the approach early Apache used with `request_rec->status`), rather than through a status return with an out-parameter, a sentinel path, or exceptions. The parser always returns a `Request` by value, and the state travels with it.
- `Request::State` has four values: `PENDING` (no verdict yet), `COMPLETE` (parse succeeded), `INCOMPLETE` (input ended before the message did; under blocking reads this is an error, but it becomes "read more bytes and parse again" when we move to an event loop), and `MALFORMED` (grammar violation; more bytes cannot repair it, respond 400).
- Legal state transitions: construction establishes `PENDING`; the parser's final act moves it to exactly one of `COMPLETE`, `INCOMPLETE`, or `MALFORMED`. `COMPLETE` and `MALFORMED` are final. In the future event-loop design, `INCOMPLETE` gains one edge back into parsing when more bytes arrive. Any other transition is a bug by definition.
- Pessimistic initialization: every `Request` is constructed in `PENDING`, and the parser promotes to `COMPLETE` only when the blank line ending the header block is actually observed. A forgotten code path therefore produces a request that is wrongly rejected (loud) instead of wrongly accepted (quiet).
- Enum zero values are the safe values: `PENDING` is first in `Request::State` and `UNKNOWN` is first in `http::Method`, so an accidental zero-initialized value fails safe at the checkpoint rather than reading as a real verdict or a real method.
- Single-writer rule: `RequestParser` is a `friend` of `Request` and is the only code that may populate fields or change the state. Everything else sees an immutable object through `const` accessors. This is enforced by the compiler, not by convention.
- Parser invariant: every `return` in `ParseRequest` is immediately preceded by a deliberate state assignment, and no path returns `PENDING`. A state assignment must always be paired with an exit (`return`), because any code that runs after a verdict gets a chance to contradict it.
- Single checkpoint rule: exactly one place (the connection-handling code, before routing) reads `request.state()` and decides what to do. `MALFORMED` maps to a 400 response there. Only a `COMPLETE` request may reach the router. A second state check anywhere downstream is a design alarm, not defensive thoroughness.
- Test fixtures are built by feeding crafted request strings through the real parser, not by constructing `Request` objects directly, so every tested state is one that is reachable in production.
- Parsed headers are stored in `Request` as the `headers_` member, using the `http::Headers` typedef (`std::map<std::string, std::string>`). The parser populates the map at parse time through the same friendship that covers the other fields.

## Response and serialization

- `Response` owns a status code, a headers map, and a body. It has a public constructor and is serialized to wire bytes by `serialize() const`, which produces the status line, headers, a blank line, and the body, with CRLF line endings throughout.
- Mutation model, deliberately asymmetric with `Request`: `Request` has one producer (the parser) and many consumers, so its single writer is compiler-enforced via `friend`. `Response` has many producers (every future handler) and one consumer (the code that writes the socket), so public construction is the honest interface and no friendship is involved.
- `Content-Length` is computed from the body at serialization time and never stored. A stored copy could disagree with the actual body, and fields derivable from other fields are derived at the moment of use.
- Status codes live in `http::StatusCode` next to `http::Method`, with enumerators assigned their explicit wire values (`HTTP_200 = 200`), so the numeric value on the wire and in the code are the same number and zero remains unclaimed by any real code, consistent with the zero-values-are-safe rule.
- Reason phrases ("OK", "Bad Request") are a pure function of the status code, provided by `http::status_code_phrase` in http.cc rather than stored per response.
- `http::StatusCode` covers the codes a basic server plausibly emits (200, 400, 403, 404, 405, 500, 501, 505); further codes are added when a code path actually produces them.
- `http::status_code_phrase` is a no-default `switch` over the enum, so `-Wswitch` (enabled by `-Wall`) flags any enumerator added without a corresponding phrase. The `return "Unknown"` after the switch handles a value cast into the enum from outside its range and deliberately does not impersonate a real reason phrase.

# Concurrency
- Implemented concurrency via `fork`. 
- `Worker::Execute` handles request processing in the child. Currently, in a class, as we may decide to add state to `Worker` eventually. An argument can be made for getting rid of the class and extracting `Execute` to a free function. 

# TBD/In-progress

- Thinking of utilizing a router approach once the request is parsed to determine how to satisfy the request.
- An unrecognized method is currently reported as `MALFORMED` (400); it may deserve its own outcome later since it properly maps to 501 Not Implemented. `HTTP_501` is now defined in the enum, so only the parser-outcome change remains.
- The version token of the request line is not yet validated.
- Header parsing still assumes exactly one space after the colon and does not lowercase key names.
- The checkpoint in main.cc is partially implemented: the `MALFORMED` branch closes the connection but does not yet send its 400 response, and `INCOMPLETE` and `PENDING` have no branches yet.
- The `Date` header value in `Response::serialize` is hardcoded scaffolding; the real implementation will use `time`/`gmtime`/`strftime` from `<ctime>` in RFC 1123 format.