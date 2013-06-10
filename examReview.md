CS244B Distributed Systems
==========================

[TOC]

Overview
------------------------

#### 3 views of Distributed Systems

##### Shared State

Shared state is necessary between processes for the processes to act as a distributed system, as opposed to independent apps. Providing shared state to application logic with suitable performance, availability, security is sufficient to allow this logic to be written with relatively little consideration of it being distributed.

##### Object

Object interfaces can define app semantics for the shared state to allow the impl to be optimized for sufficient consistency, improving performance and availability.

##### Protocol - syntax, semantics, timing

The wire protocol needs to be carefully defined to achieve/maintain interoperability, a key issue as the system evolves. It is also a key determinant of performance and fault-tolerance. If you are magically given the perfect wire protocol, local programming screw-ups are a small matter of programming to recover from. If you have a bad wire protocol, is a system-wide and hard problem to recover from.

#### Coupling

+ the level of access to state: lower-level access ->  tighter coupling
+ strictness of consistency: strict consistency -> tighter coupling
+ communication performance requirements: low latency comm -> tighter coupling

#### Trust - the expectation of future good behavior (and the absence of bad behavior)

established and maintained in three key ways:

+ Observation of good behavior by the principal in the past.
+ Attestation of the trustworthiness of the principal by another trusted principal.
+ Absence of complaints about the principal.

Two key requirements:

+ Identification: a principal can only be trusted if it can be reliably and consistently identified.
+ Observability: a principal can only be trusted if its behavior can be observed.

#### Why build DS?

+ composition
+ decomposition
+ replication

#### Challenge for building DS
+ more complex and general
+ more complex failure modes
+ very large-scale design with evolution over long times and never down or fully recompiled

Computer node:

+ processor
+ memory
+ network interface

Properties of interconnection network:

+ data rate
+ delay
+ reliability/availability

#### Issue to consider when building DS software

+ Modularity
    * What functionalities, state and properties are sensible to encapsulate in a module and what functionalities need to be provided as part of multiple modules(possibly as a common library)?
    * In what module should the state and functionality be placed?
    * How should that module interact or coupling with others?
+ network transparency
+ fault tolerance
+ scalability
+ admin, management
+ fate-sharing


---------

Distributed shared memory
------------------------

#### Consistency
A shared memory is consistent if there exists some total ordering of all the performed memory operations such that for each client of the memory the ordering is consistent with the partial ordering that each node observes.

#### DSM vs SMMP
+ DSM can share partial memory
+ DSM has out of order update message
+ No reliable broadcast facility in DSM

#### DSM issues

+ **False sharing**
    * **Three units in cache**
        * consistency unit
        * addressing/mapping unit
        * transfer unit
+ **Program restructuring**
    * Align data to consistency unit boundary
    * minimize/localize write shared data
    * read ahead to minimize latency
+ **Logical and physical contention control** build lock into control
+ **heterogeneity** endianess
+ **fault tolerance** one bad node brings down all nodes
+ **security** update is observable

#### Special consistency protocol
+ Release consistency - consistent after release lock
+ sequential consistency
+ message-oriented consistency - consistency after getting signal on memory region

#### Direct comm
Advantage

+ simple and fast in simple cases

Disadvantage

+ source must know recipients
+ source and receiver must co-exist
+ source does not know how often to send
+ low-level code to interface message. pollute application logic
+ hard to debug

---------

Distributed System Structuring
------------------------------

#### Why function shipping?
+ Performance: remote site has code cached
+ special remote hardware resource
+ special remote software(licensed)

#### Distributed by message
Advantage

+ Close to hardware base(packet)
+ flexible: async from process; can use multicast (not possible if procedure call)
+ synchronization: synced on explicit receive call

Disadvantage

+ awkward marshalling, not type safe

Why streaming is bad? lose record/packet boundary.

#### Distributed by file
Disadvantage

+ File is clumsy and error-prone
+ performance: overhead to produce file format
+ consistency: only coarse-grain or time-based consistency

#### OOP
key aspects in OOP

+ uniform reference
+ clients access through interface
+ inheritance mechanism by extending interface

#### OO RPC
Advantage:

+ Provides familiar programming abstraction - i.e. procedure call. Raw messages and socket communication are low level tools that are hard to reason about. 
+ Reduced code to write: Stub compilers for OO RPC give you serialization and deserialization basically for free, and handles issues like endianness.
+ **Network transparency**: Interfaces separate from implementation: OO RPC lets you separate the interface you're calling from its implementation, which is often on the other side of a network.

Disadvantage:

+ Performance, unbounded delay
+ at most/at least once semantics
+ Couple with programming language
    * Marshaling complex data structure
    * procedure call does not fit with multicast/streaming model

#### Smart proxy
Separate interface from protocol. Why good?

+ high extra cost over network
+ use client CPU instead of stressing server
+ No need to save memory, because memory are cheap now

##### Manual stub implementation

Advantage:

+ can apply optimization
+ avoid dependence on stub compiler
+ more portable and traceable without stub compiler

Disadvantage: more programming. But not that bad, because:

+ code is relatively small
+ stabilize early
+ simple case code straightforward and complex case stub compiler cannot help anyway 

#### Cheriton’s Stateful proxy
Advantage:

+ Easy recovery. Just resync states 
+ Better performance. Read/Write is local. Update is async
+ Loose coupling to programming language

Disadvantage:

+ restricting interface to just attribute
+ large overhead to create interface for small objects
+ Creating proxy has high RTT


----------


Clocks
-----------

#### Type of Clocks
+ Real clock - constant rate, monotone increase
+ Virtual clock - variable rate, monotone increase
+ Logical clock - increase after each event

#### happened-before relation:
+ If events A and B occur on the same process, A--B iif the occurrence of event A preceded the occurrence of event B
+ If event A is the sending of a message and event B is the reception of the message sent in event A, then A--B


#### Sufficient consistency for real time clock
$$|C_i - C_j| < \delta$$
$$|C_i-RealTime|< \Delta$$

#### Management issue
+ overloading of the highest stratum NTP sources
+ security issue of whether you can trust the time being reported by the server(s) you connect to
+ bootstrap issue of which server(s) you connect to when you start up

#### NTP Scaling
+ Hierarchy
+ Dead reckoning
+ Multicast optimization

#### Cheriton's view
+ Already trust the infrastructure, use router
+ No more peering

#### CATOCS

Disadvantages

+ can't say for sure
    * hidden channel(DB), external events(fire)
+ can’t say together
    * still need locking for grouping
    * 2PC cannot reject
+ can’t say the whole story
    * cannot enforce serializability
+ can’t say efficiently
    * message overhead / false causality / no efficiency gain / scalibility



-------------

Naming & Directory
-------------------
#### Definitions

directory
:    an object that maps names to descriptions, identifiers or references.

name
:    a value used to refer to or identify an object or collection of objects.

proper name
:    denotes but lacks connotation or meaning.

common name
:    a symbolic reference to a definition or description.

pure name
:    one that contains no encoding of a description of an entity.

#### Why names

+ Correctness: A (proper) name identifies a specific entity unambiguously whereas there may be multiple entities matching a description. Moreover, multiple copies of descriptions lead to inconsistencies when object is modified.
+ Efficiency: names are shorter, easier to manipulate than entities named.
+ Flexibility: provides a level of indirection, allowing rebinding.

#### Impure name

Impure name includes some description

Advantage

+ encode information to facilitate name mapping(area code)
+ provide holder of name with information about entity named without having to access object description
+ facilitates distributed allocation because of encoded info.

Disadvantage

+ larger size
+ restricted rebind(move between area code)

#### External VS Internal name

External names are a necessity; internal names are an optimization.

Why external name

+ referring to objects from the command line, scripts and GUI menus.
+ meaningful error reporting
+ address-space and machine-independent identification of objects

Why internal name

+ efficient, simpler machine code
+ only needed in context, can be transient

#### Naming operations

+ create an object with a new name or, create a new (alias) name for an existing object.
+ map a given name to the object it names.
+ invalidate a name.

#### UUID harmful

+ How big? 
+ Big UUID is expensive to allocate and map. (impure makes it even bigger)
+ need external server to resolve(off-path dependency)
+ hard to assign UUID to existing objects
+ when to assign new UUID when modified

#### Centralized versus Decentralized Naming

Centralized Pros and Cons

+ (+) one authority to consult
+ (+) more efficient in communication
+ (+) no inconsistent responses
+ (-) awkward to extend in distributed systems.

Decentralized Pros and Cons

+ (-) more expensive to communicate with all authorities
+ (-) deal with multiple responses
+ (+) easily distributed and robust.

#### External naming implementations
##### Lampson’s Global Name Service

Disadvantage

+ Too inefficient for file naming, etc.
+ No support for locality.
+ No support for user customization.


##### Epidemic Approach

Disadvantage

+ when should the rumor mongers lose interest in the rumor
+ missing someone
+ need to exploit hierarchy for efficient infection, but defeat the purpose


##### V-System

Advantage

+ efficiency — eliminate going through name server on each access.
+ consistency — external name and object in same module.
+ fewer levels of naming — less need for global (internal) identifiers.
+ extensibility — can handle established name spaces.
+ availability — external name for object available if object is, i.e. fate sharing.
+ security — name operations use same security mechanism as provided for accessing server/manager for other operations.

Disadvantage

+ cannot distinguish ‘undefined’ from ‘not available’ or ‘not reachable’,
+ cost to multicast

##### Security with Decentralized Naming

Solutions

+ Client knows the principals authorized for each portion of name space.
+ authenticator or capability from trusted 3rd party
+ servers advertise their services periodically in terms of name space coverage. Use social fabric for trust.

#### URL

Advantage

+ represents the brand associated with the information
+ represent a logical or virtual server, not necessarily a single physical server
+ use redirect to support migration of subtrees that are below the unit of web site.

Disadvantage

+ splitting a website

#### Location-based naming

Advantage

+ support or undermine trust in actions in many situations.
+ valuable as a bootstrapping mechanism to establish trust in identity.
+ depend on physical properties which the intruder cannot compromise.

---------

Account & Authentication
------------------------
#### Encryption

+ Confidentiality
+ Integrity
+ Authentication
+ Non-repudiation

#### Secrecy

Disadvantage

+ The secret is generally a single point of failure. 
+ There is no way to test or prove that some value is a secret.
+ there is no way to determine when the system has failed. 
+ Does not scale

#### Key management issue

+ Lifetime(tomato model)
+ Selection
+ Distribution
    * **external channel:** high overhead
    * **chained key distribution:** once key is known, can get future keys
    * **two-level key distribution:** key distribution key is compromised

#### Authentication Schemes

+ Principal supplies password, key or authenticator as proof.
    * human write it down, simple
    * sent over secure channel, recursive problem
+ Principal encodes or encrypts or decrypts data such that it proves identity.
    * replay -> use non-reused identifier
+ A third trusted party vouches for the principal.


#### PKI Certificates Considered Harmful

+ wait for revocation to propagate
+ does not scale because they still depend on the private key for the root CA
+ liability is unclear
+ customer preception: how much? fee type?

#### Relative authentication

Compare NTP

+ the local level may use a periodic multicast of the authentication server’s public key to ensure that this key is well-known locally. 
+ hop-by-hop authentication along a hopefully trusted path
+ the authentication can use multiple paths to authenticate for reliability

Scalability concern

Small world model ensure small hops(degree of sepration)


#### Open Security, Physical Security

+ confidentiality often not needed, eg water company
+ detect compromises quickly
+ uses redundancy in server/router to handle failures

> need to force multicast

#### Accounting

Usage based accounting disadvantage

+ warp user behavior
+ incurs a significant resource overhead, and often a significant personnel overhead to administer
+ many basic costs of computing services are not incurred based on use but based on a fixed cost over time



----------


Transactions
----------------

#### A distributed object

+ Each transaction is represented by an object that has a unique identifier.
+ The transaction object has an interface that supports commit, abort, query operations on the transaction.
+ The transaction object incorporates and encapsulates state, such as a list of participants and log records.

Advantage

+ Maintain consistency
+ simplify concurrency control, ie central for locking and undoing
+ Solves priority inversion by rolling back lower priority process
+ deadlock resolution
+ efficiency benefit as batching



#### Case study OOTP

TID=`(timestamp, channelId, localId)`

Advantage

+ Avoid allocation having to be system-wide unique.
+ provides an efficient form of communication with the participating servers.
+ The localId allows efficient TID generation for participating servers and avoids having a channel per transaction


#### Detecting failure
Use **ping** or **heartbeat message**, but what to do when node is under load?

+ keep alive is high priority
+ avoid fix timeout

> Use low level detection can violate end-to-end argument. Application died, but ICMP alive.


#### Asynch stateful proxies with transactional behavior

+ many applications fit this model well
+ providing better semantics can occur an unbounded space cost, possibly
leading to other failures.
+ there is significant code complexity required to provide these better semantics, especially true atomic update semantics.

##### provide consistency

+ **read-only clients:** updates can be generated in order so the reader/receiver
can collect up a sequence of updates into a snapshot that corresponds to the atomic
update it would have received in a strict transactional system, being activated by a
trigger attribute.
+ **writers:** there can be a consistent global ordering on state updates, given that writers to given state are serialized through an updater. Also, the write updates from a given writer are fix-ordered, especially if the normal update order is the same as the attribute order.
+ **update interface** can lock all the data required by the writer to perform a consistent update as well as create snapshots.


----------


Examples
---------------
+ mazewar
+ replicated Filesystem
+ Sensor network
+ real time video
+ NFS
+ stock price inconsistency
+ Air traffic control
