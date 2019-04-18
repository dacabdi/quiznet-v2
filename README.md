CNT4007C - **Project 2**
02/18/2019
David Cabrera
dacabdi89@ufl.edu

**NOTE:** This PDF file was automatically generated from the README.md file of the **private** GitHub repository that hosts the code.

# QUIZNET #

---

## USAGE ##

### BUILDING ###

To build the solution you can either,

- `$ make cserver`,
- `$ make contestmeister`,
- `$ make contestant`,
- or `$ make all` to build all applications.

The `makefile` provides comprehensive recipes to run tests, compile in debug mode with no optimizations, or produce manual entries. To run in debug mode set `MODE=debug` before calling on `make`. See `makefile` for more details.

### RUNNING ###

Please note that all three applications have been implemented using POSIX command line options with `getopt` and `getopt_long`. Please use options `-h` or `--help` on each executable for further usage details.

#### `cserver` ####

After building the solution, you can run the _contest server_ application by issuing the command `$ ./cserver [-p PORT]` standing on the root subdirectory of the project. You can pass the `-p PORT` option and specify the port where you which to run the server on, otherwise it will obtain a randomly assigned port.

The server application provides logging and verbosity options. Run with `--verbose` or `-v` for complete escaped reporting of serialized representations of both requests and responses.

#### `contestmeister` ####

The _contestmeister_'s application can be started by issuing the command, `./contestmeister TARGETHOST PORT`. Where the `TARGETHOST` and `PORT` will indicate the server over which the contestmeister will exert arbitrage.

#### `contestant` ####

The _contestant_ can be started by issuing the command, `./contestant TARGETHOST CONTESTPORT`. Where the `TARGETHOST` will be that of the contest server and the `CONTESTPORT` will match the port assigned to the ongoing contest the contestant whishes to join.

---

## PROTOCOL SPECIFICATION ##

Before any request or response, the _contestant_ and the _contestmeister_ MUST perform an application level _handshake_ by which it gives notice to the _server_ on the type of session it wishes to establish, as well as any other relevant information required to maintain the communication.

Both types of clients should initiate the handshake on the FIRST request made to the server for each connection. It will send a type `s` request containing the parameters and identifying information in the `body`.

### REQUESTS ###

If any of these requests is sent to the server without first negotiating and identifying the session, the server will reject the petition and return a `NOAUTH` error response.

#### GENERAL REQUEST/RESPONSE SPECIFICATION ####

All _Request_ and _Response_ messages must adhere to the following syntactic specification

```text
type length\n
body
```

|Field|Syntax|Semantics|Optional|
|---|---|---|---|
|`type`|Only **one** character from a-z|Indicates the type of request|No, all requests must have a `type` field|
|`length`|A decimal non-negative integer|Describes the length of the request's `body` in bytes|No, an empty request is expected to include a 0 length|
|`body`|Any number of ASCII characters|Request's body and payload|Yes, but 0 length must be indicated|

More precisely,

```EBNF
request = type , ' ' , length, '\n' , (body)?
        ;
type    = a-z
        ;
length  = [0-9]+
        ;
body    = ? any sequence of chars ?
        ;
```

If the server _knows_ how to handle the incoming type of request and the request is processed successfully, a type `o` OK response will be sent to the client carrying any requested data.

Otherwise, a type `e` ERROR response will be returned indicating the reason for failure. If the error is due to an unrecognized type of request, the error type will be numbered `UNKREQ`.

#### TYPES OF REQUESTS ####

##### HANDSHAKE #####

Also know as _greeting_ or _session negotiation_ this is the FIRST request made over each session for any type of client. It lets the server get ready for the upcoming requests.

```plain-text
n length\n
client-type\n
parameters\n
```

###### Contestant Handshake ######

For a regular _contestant_, the server expects the client to provide a `nickname` in the request. 

```plain-text
n length\n
contestant\n
nickname\n
```

If the `nickname` is not already in use for the current contest, the server will provide an `OK` response containing some other miscellaneous information, such as the time left before the contests starts, the number of questions, etc.

```plain-text
o length\n
no-of-questions\n
[...]
```

After a successful handshake, the client application will wait until the server starts the contest and the questions are provided. Each round will contain one question and the server won't provide the next question until all _contestants_ have provided an answer. At that point the statistics will be shown to the _contestant_ and the next question, if any, will be presented.

If the `nickname` is already in use, the server will reply with a `NCKDUP` error message. If the contest has already started (i.e, the port exists but the client is late), the server will respond with a `LATEJN` error. Any other unspecified error is likely to provide further information by wrapping the inner exception in the `extra` field of the error response.

###### Contestmeister Handshake ######

For a _contestmeister_, this handshake will simply setup a session.

```plain-text
n length\n
contestmeister\n
```

Any unspecified error is likely to provide further information by wrapping the inner exception in the `extra` field of the error response.

##### ANSWER #####

Used by a _contestant_ to submit the answer to a question provided by the server.

```plain-text
w length\n
choice\n
```

On success, the server will respond with an `OK` response indicating the result, stats, and the number of questions left.

```plain-text
o length\n
result\n
percentage-right,contestant-score,top-score\n
questions-left\n
```

Expect a `CHNFND` error if choice does not exist. In such case, **the server will expect another `ANSWER` request**. Until an incorrect but existing choice, or a correct choice is provided, the server will remain looping in this state.

Any unspecified error is likely to provide further information by wrapping the inner exception in the `extra` field of the error response.

##### POST #####

Upload a question to the server

```plain-text
p length\n
body\n
```

This type of request must include a `length` field and a `body` field containing,

```text
id\n
tags\n
question-title\n.\n
choices.\n
(correct-answer)\n
```

1. `id` numeric field with proposed question id
2. `tags` field with zero or more comma separated tags.
3. `question-title` with a string of alphanumeric and white space characters terminated by the sequence `\n.\n`.
4. Any number of `choices` of the form `(choice-character) choice-text\n.\n` followed by an additional `\n.\n` that ends the section.
5. The correct answer `correct-answer` followed by an end of line.

Please observe that the server will enforce the following rules

1. The id must not be already in use.
2. A question must contain at least 2 choices.
3. Every choice must be of the format `(choice-character) choice-text\n.\n`.
4. The `choice-character` value is a unique a-z character, the choices must be in alphabetic order, and the first choice must be `a`.
5. The `correct-answer` provided must exist in the set of choices.

Upon success, the server will reply with an `OK` containing the id of the newly posted question on the body of the response. Expect an error response if one or more of these rules are violated. Specifically, an attempt to post a question with an id that already exists will return a `USEDID` error. Any other violation is likely to provide further information by wrapping the inner exception in the `extra` field of the error response.

##### DELETE #####

Request a deletion using type `d` and indicating the question's identifier in the body of the message.

```plain-text
d length\n
question-id\n
```

An `OK` response containing the id of the deleted question will be issued upon success. Expect an `NOTFND` error response if question doest not exist, or a `INVQID` if the the question id is not in a valid format (i.e., not a numeric value).

##### GET #####

Request a question using type `g` and indicating the question's identifier in the body of the message.

```plain-text
g length\n
question-id\n
```

A positive response will consist of a regular `OK` response containing a serialization of the question as specified in the `POST` request. That is, the body of the response will be formed as follows,

```plain-text
id\n
tags\n
question-title\n.\n
choices.\n
correct-answer\n
```

Expect an `NOTFND` error response if question doest not exist, or a `INVQID` if the the question id is not in a valid format (i.e., not a numeric value).

##### CHECK #####

Check if an specific choice is the valid response to a question identified by its question id.

```plain-text
c length\n
question-id\n
choice\n
```

An `OK` response containing the result in the `body` field will be issued back. Expect a `NOTFND` error if question does not exist or a `CHNFND` if the question does not contain the provided choice.

##### KILL #####

Kill server process.

```plain-text
k 0\n
\n
```

Will respond `OK` before going down. If not issued by a client that previously identified as a _contestmeister_, an error response will be issued.

##### QUIT #####

Inform the server that the client wishes to leave.

```plain-text
q 0\n
\n
```

If issued by a _contestmeister_, the server must gracefully drop the contest's socket after receiving this request. If issued by a regular contestant, just gracefully close the connection socket.

##### REVIEW #####

Review an specific contest session.

```plain-text
r length\n
contest-id\n
\n
```

An corresponding `OK` response will have the following syntax,

```plain-text
o length\n
number-of-questions\n
c-run-flag,average-correct,maximum-correct\n
question-id,percent-right\n
```

Expect an error `CTNFND` indicating that the contest was not found if the contest id does not match any current contest on the server. Any other error is likely to provide further information by wrapping the inner exception in the `extra` field of the error response.

##### SET #####

Create a new contest with contest id `contest-id`.

```plain-text
s length\n
contest-id\n
\n
```

An `OK` response will follow the following syntax,

```plain-text
o length\n
contest-id\n
```

Expect an error `USEDID` indicating that the contest id is already in use if such situation arises. Any other error is likely to provide further information by wrapping the inner exception in the `extra` field of the error response.

##### ADD #####

Add an existing question to an existing contest.

```plain-text
a length\n
contest-id,question-id\n
\n
```

An `OK` response will follow the following syntax,

```plain-text
o length\n
contest-id question-id\n
```

Expect an error `CTNFND` indicating that the contest was not found if the contest id does not match any current contest on the server. Similarly, expect a `NOTFND` if the question id does not exist in the server's bank. Any other error is likely to provide further information by wrapping the inner exception in the `extra` field of the error response.

##### BEGIN #####

Start a previously registered contest identified by `contest-id`.

```plain-text
s length\n
contest-id\n
\n
```

Upon receiving this request, the server will open a random port and map it to the `contest-id`. If successful, the server will then issue a `OK` response indicating the newly opened port. The `OK` response will follow the following syntax,

```plain-text
o length\n
contest-id,contest-port\n
```

The new port will listen for incoming connections from _contestants_ for a window of **1 minute**. Once the waiting period has expired the server will stop taking contestants in that port and start the contest. 

Expect an error `CTNFND` indicating that the contest was not found if the contest id does not match any current contest on the server. Any other error is likely to provide further information by wrapping the inner exception in the `extra` field of the error response.

##### LIST #####

List all contests.

```plain-text
l 0\n
\n
```

An corresponding `OK` response will have the following syntax,

```plain-text
o length\n
number-of-contests\n
c-id,no-questions,c-run-flag,average-correct,maximum-correct,running-port\n
[...]
```

Notice that the `running-port` value is optional and will be set to zero (i.e., `0`) if the contest is not currently running, (i.e., the `c-run-flag` is not set, that is, it is equal to `0`).

Expect an error `CTNFND` indicating that the contest was not found if the contest id does not match any current contest on the server. Any other error is likely to provide further information by wrapping the inner exception in the `extra` field of the error response.

##### TERMINATE CONTEST #####

Tell client to end contest.

```plain-text
t length\n
[stats]\n 
```
TODO! write this!

##### GENERAL MESSAGE #####

Used for general non-specific communication. Since a contest participation has a predetermined flow, this kind of message will be used mainly for the contestant to participate in a contest.

```plain-text
u length\n
content\n
```

---

### RESPONSES ###

All responses will come in two types, OK (type `o`) responses indicating a successful request and ERROR (type `e`) responses along with information on the failure.

#### TYPES OF RESPONSES ####

##### OK #####

```plain-text
o length\n
body\n
```

The content for the body of the response will depend on the particular request that caused the response.

##### ERROR #####

Error responses along with error codes and extra information.

```plain-text
e length\n
[number]\n
[symbol]\n
[message]\n.\n
[extra]\n.\n
```

The **optional** `extra` field will most likely contain the message generated by the inner exception that triggered the error response. It will be useful for debugging, unexpected errors, and extra circumstantial information.

|no|symbol|message|extra|
|---|---|---|---|
| 0|`UNKERR`|Unknown error|Inner exception message, if any|
| 1|`NOTFND`|Question not found|`[question-id]` provided|
| 2|`MALQUE`|Malformed question body|Exception message, if any|
| 3|`CHNFND`|Choice does not exist in question|`[choice]` and `[question-id]`|
| 4|`UNKREQ`|Server does not know how to handle request type|Received request type|
| 5|`INVQID`|Invalid format for question id provided|Provided id|
| 6|`EMPTYQ`|Empty quiz book|None|
| 7|`REQTSZ`|Request type length must be one character|Type provided|
| 8|`USEDID`|The proposed id is already in use|Provided id|
| 9|`CTNFND`|Contest not found|Provided contest id|
|10|`LATEJN`|Contest has already started|Provided contest id|
|11|`FAILGT`|Failed to negotiate session|Inner exception message, if any|
|12|`MSGDSL`|Message deserialization error|Inner exception message, if any, and passed string to serialize|
|13|`NCKDUP`|Duplicate nickname provided|Duplicate nickname|
|14|`EMPTYN`|Empty nickname provided||

---

## IMPLEMENTATION ##

### General description ###

The solution was implemented in C++. Wrappers interfaces, and data models were written around the networking routines and the data representation. This approach allowed to develop the application abstracting the data management and ensuring integrity across the code.

The implementation is divided in three major areas, networking, data models, and other types (exceptions, escaping functions, etc).

### Networking ###

Objects `Socket` and `Host` provide wrappers with all types of operations around the file descriptors and hosts representations used by the operating system. Using this abstractions allowed to gain some leverage over the complexity of the structures used at the Operating System level.

#### `QuizServer` ####

`QuizServer` is an abstraction of the set of services provided by the server application. Upon construction it receives it receives a host
host representation and a socket object to work with. Internally, when the method `run` is called, the object takes care of the typical initialization routing of the `Socket` object, namely, it calls methods that wrap around `bind`, `listen`, and `accept`. 

Internally, the object uses a hash map pairing characters to callables. This approach allowed to drop new request/response procedures at-will if the specifications of the protocol so required. That is, the procedure that takes care of forming the response to a type `p` request, is a callable keyed on the char `'p'` of the `_handlers` map. Similarly, each corresponding procedure takes in a `Request` object and produces a `Response` object that is then serialized and sent back to the client. The handlers are typically _lamba expressions_, this provides _ad-hoc_ request handling emplacement.

If any error were to occur, it is properly identified and reported back to the client by building a type `e` `Response` containing, usually, content that mirrors the exception that was raised on the server side.

`QuizServer` also takes a `QuizBook` object with a data model representation of the questions read/written to/from the question bank file (refer to the section 5 of the manual for file format information). All the entity manipulations are mediated by the `QuizBook` interface.

`QuizServer`, as per the protocol specifications, expects the client to perform a brief handshake, where it specifies the type of connection it desires to establish. This handshaking can be used in the future to communicate other relevant parameters.

#### `QuizClient` ####

On the other hand, the `QuizClient` utilizes a similar pattern of matching characters to callables. Each handler will take in the line and data introduced over standard input and build a `Request` object to relay back to the server. Before returning the object, it sets a `std::function` typed method with a _lambda expression_ that will handle the response once the servers replies.

`QuizClient` will create a `Socket` object if it functions on _persistent_ mode. Otherwise, it will instantiate a new one over each request.

Please notice that `QuizClient` performs a brief application level handshake (or session negotiation) with the server to give notice of the connection mode and other relevant information.

### Data Models ###

For every component of a question, the question itself, and the entire quiz book, objects defining its interfaceable contracts were implemented. Using this approach the serialization and deserialization of the models became a centralized issue, facilitating bug tracking and standardization.

Each object validates its input and enforces data requirements. All cases provide overloaded stream operators to allow easy output, verification, and serialization.

To avoid data loss on server crashes, the `QuizBook` works on the file using a writethrough approach.

Data models include `Tag`, `TagCollection`, `Choice`, `ChoiceCollection`, `Question`, `QuestionSolved` (containing a question along with a verified solution), etc.

### Exceptions ###

Customized exception objects have been provided for `ProtocolException` and `NetworkException`. These exception objects provide granularity in the catching of errors, allowing to distinguish between the two types of problems. Both client and server are robust and can survive (graciously) the other party leaving the session. These endpoints will use protocol specifications to communicate each other exceptions and errors and handle them appropiately. Refer to the _Protocol Specification_ session for more details.

#### ADDITIONAL FEATURES ####

Although the specifications did not require implementing both persistent
and non persistent mode, the client takes a flag `-n` or `-nonpersistent`
to allow non persistent mode.