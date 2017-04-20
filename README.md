CKPOOL + CKDB + libckpool by Con Kolivas and Andrew Smith.

Ultra low overhead massively scalable multi-process, multi-threaded modular
bitcoin mining pool, proxy, passthrough, library and database interface in c
for Linux.

CKPOOL is code provided free of charge under the GPLv3 license but its development
is mostly paid for by commissioned funding, and the pool by default contributes
0.5% of solved blocks in pool mode to the development team. Please consider leaving
this contribution in the code if you are running it on a pool or contributing to the
authors listed in AUTHORS if you use this code to aid funding further development.

---
LICENSE:

GNU Public license V3. See included COPYING for details.


---
DESIGN:

Architecture:
- Low level hand coded architecture relying on minimal outside libraries beyond
basic glibc functions for maximum flexibility and minimal overhead that can be
built and deployed on any Linux installation.
- Multiprocess+multithreaded design to scale to massive deployments and
capitalise on modern multicore/multithread CPU designs.
- Minimal memory overhead.
- Utilises ultra reliable unix sockets for communication with dependent
processes.
- Modular code design to streamline further development.
- Standalone library code that can be utilised independently of ckpool.
- Same code can be deployed in many different modes designed to talk to each
other on the same machine, local lan or remote internet locations.

Modes of deployment:
- Comprehensive pooled mining solution with a postgresql database interface.
- Passthrough node(s) that combine connections to a single socket which can
be used to scale to millions of clients and allow the main pool to be isolated
from direct communication with clients.
- Proxy nodes with a database that act as a single client to the upstream pool
while storing full client data of their own.
- Simple proxy without the limitations of hashrate inherent in other proxy
solutions when talking to ckpool.
- Simple pool without a database.
- Library for use by other software.

Features:
- Bitcoind communication to unmodified bitcoind with multiple failover to local
or remote locations.
- Local pool instance worker limited only by operating system resources and
can be made virtually limitless through use of multiple downstream passthrough
nodes.
- Proxy and passthrough modes can set up multiple failover upstream pools.
- Optional share logging.
- Virtually seamless restarts for upgrades through socket handover from exiting
instances to new starting instance.
- Configurable custom coinbase signature.
- Configurable instant starting and minimum difficulty.
- Rapid vardiff adjustment with stable unlimited maximum difficulty handling.
- New work generation on block changes incorporate full bitcoind transaction
set without delay or requiring to send transactionless work to miners thereby
providing the best bitcoin network support and rewarding miners with the most
transaction fees.
- Event driven communication based on communication readiness preventing
slow communicating clients from delaying low latency ones.
- Stratum messaging system to running clients.
- Accurate pool and per client statistics.
- Multiple named instances can be run concurrently on the same machine.


---
BUILDING:

Building ckpool standalone without ckdb has no dependencies outside of the
basic build tools on any linux installation.

sudo apt-get install build-essential yasm
./configure --without-ckdb
make


Building with ckdb requires installation of the postgresql, gsl and ssl
development libraries.

sudo apt-get install build-essential yasm libpq-dev libgsl-dev
./configure
make

older distributions may instead require a different version of gsl:
 sudo apt-get install build-essential libpq-dev libgsl0ldbl libgsl0-dev yasm

N.B. ckdb also requires libssl-dev but libpq-dev depends on it and installs it


Building from git also requires autoconf and automake

sudo apt-get install build-essential yasm libpq-dev libgsl-dev autoconf automake libtool
./autogen.sh
./configure
make


Binaries will be built in the src/ subdirectory. Binaries generated will be:
ckpool - The main pool back end
ckdb - The pool's database
ckpmsg - An application for passing messages in libckpool format to ckpool/ckdb
notifier - An application designed to be run with bitcoind's -blocknotify to
	notify ckpool of block changes.


Installation is NOT required and ckpool can be run directly from the directory
it's built in but it can be installed with:
sudo make install


It is anticipated that pool operators wishing to set up a full database based
installation of ckpool+ckdb will be familiar with setting up postgresql and
associated permissions to the directories where the various processes will
communicate with each other and a web server so these will not be documented.


---
RUNNING:

ckpool supports the following options:

-A | --standalone
-c CONFIG | --config CONFIG
-d CKDB-NAME | --ckdb-name CKDB-NAME
-g GROUP | --group GROUP
-H | --handover
-h | --help
-k | --killold
-L | --log-shares
-l LOGLEVEL | --loglevel LOGLEVEL
-N | --node
-n NAME | --name NAME
-P | --passthrough
-p | --proxy
-R | --redirector
-S CKDB-SOCKDIR | --ckdb-sockdir CKDB-SOCKDIR
-s SOCKDIR | --sockdir SOCKDIR
-u | --userproxy


-A Standalone mode tells ckpool not to try to communicate with ckdb or log any
ckdb requests in the rotating ckdb logs it would otherwise store. All users
are automatically accepted without any attempt to authorise users in any way.
This option is explicitly enabled when built without ckdb support.

-c <CONFIG> tells ckpool to override its default configuration filename and
load the specified one. If -c is not specified, ckpool looks for ckpool.conf,
in proxy mode it looks for ckproxy.conf, in passthrough mode for
ckpassthrough.conf and in redirector mode for ckredirector.conf

-d <CKDB-NAME> tells ckpool what the name of the ckdb process is that it should
speak to, otherwise it will look for ckdb.
This option does not exist when built without ckdb support.

-g <GROUP> will start ckpool as the group ID specified.

-H will make ckpool attempt to receive a handover from a running incidence of
ckpool with the same name, taking its client listening socket and shutting it
down.

-h displays the above help

-k will make ckpool shut down an existing instance of ckpool with the same name,
killing it if need be. Otherwise ckpool will refuse to start if an instance of
the same name is already running.

-L will log per share information in the logs directory divided by block height
and then workbase.

-l <LOGLEVEL will change the log level to that specified. Default is 5 and
maximum debug is level 7.

-N will start ckpool in passthrough node mode where it behaves like a
passthrough but requires a locally running bitcoind and can submit blocks
itself in addition to passing the shares back to the upstream pool. It also
monitors hashrate and requires more resources than a simple passthrough. Be
aware that upstream pools must specify dedicated IPs/ports that accept
incoming node requests with the nodeserver directive described below.

-n <NAME> will change the ckpool process name to that specified, allowing
multiple different named instances to be running. By default the variant
names are used: ckpool, ckproxy, ckpassthrough, ckredirector, cknode.

-P will start ckpool in passthrough proxy mode where it collates all incoming
connections and streams all information on a single connection to an upstream
pool specified in ckproxy.conf . Downstream users all retain their individual
presence on the master pool. Standalone mode is implied.

-p will start ckpool in proxy mode where it appears to be a local pool handling
clients as separate entities while presenting shares as a single user to the
upstream pool specified. Note that the upstream pool needs to be a ckpool for
it to scale to large hashrates. Standalone mode is Optional.

-R will start ckpool in a variant of passthrough mode. It is designed to be a
front end to filter out users that never contribute any shares. Once an
accepted share from the upstream pool is detected, it will issue a redirect to
one of the redirecturl entries in the configuration file. It will cycle over
entries if multiple exist, but try to keep all clients from the same IP
redirecting to the same pool.

-S <CKDB-SOCKDIR> tells ckpool which directory to look for the ckdb socket to
talk to.
This option does not exist when built without ckdb support.

-s <SOCKDIR> tells ckpool which directory to place its own communication
sockets (/tmp by default)

-u Userproxy mode will start ckpool in proxy mode as per the -p option above,
but in addition it will accept username/passwords from the stratum connects
and try to open additional connections with those credentials to the upstream
pool specified in the configuration file and then reconnect miners to mine with
their chosen username/password to the upstream pool.


ckdb takes the following options:

-b DBPREFIX | --dbprefix DBPREFIX
-c CONFIG | --config CONFIG
-d DBNAME | --dbname DBNAME
-h | --help
-k | --killold
-l LOGLEVEL | --loglevel LOGLEVEL
-n NAME | --name NAME
-p DBPASS | --dbpass DBPASS
-r CKPOOL-LOGDIR | --ckpool-logdir CKPOOL-LOGDIR
-R LOGDIR | --logdir LOGDIR
-s SOCKDIR | --sockdir SOCKDIR
-u DBUSER | --dbuser DBUSER
-v | --version
-y | --confirm
-Y CONFIRMRANGE | --confirmrange CONFIRMRANGE


ckpmsg and notifier support the -n, -p and -s options

---
CONFIGURATION

At least one bitcoind is mandatory in ckpool mode with the minimum requirements
of server, rpcuser and rpcpassword set.

Ckpool takes a json encoded configuration file in ckpool.conf by default or
ckproxy.conf in proxy or passthrough mode unless specified with -c. Sample
configurations for ckpool and ckproxy are included with the source. Entries
after the valid json are ignored and the space there can be used for comments.
The options recognised are as follows:


"btcd" : This is an array of bitcoind(s) with the options url, auth  and pass
which match the configured bitcoind. The optional boolean field notify tells
ckpool this btcd is using the notifier and does not need to be polled for block
changes. If no btcd is specified, ckpool will look for one on localhost:8332
with the username "user" and password "pass".

"proxy" : This is an array in the same format as btcd above but is used in
proxy and passthrough mode to set the upstream pool and is mandatory.

"btcaddress" : This is the bitcoin address to try to generate blocks to.

"btcsig" : This is an optional signature to put into the coinbase of mined
blocks.

"blockpoll" : This is the frequency in milliseconds for how often to check for
new network blocks and is 100 by default. It is intended to be a backup only
for when the notifier is not set up and only polls if the "notify" field is
not set on a btcd.

"nodeserver" : This takes the same format as the serverurl array and specifies
additional IPs/ports to bind to that will accept incoming requests for mining
node communications. It is recommended to selectively isolate this address
to minimise unnecessary communications with unauthorised nodes.

"nonce1length" : This is optional allowing the extranonce1 length to be chosen
from 2 to 8. Default 4

"nonce2length" : This is optional allowing the extranonce2 length to be chosen
from 2 to 8. Default 8

"update_interval" : This is the frequency that stratum updates are sent out to
miners and is set to 30 seconds by default to help perpetuate transactions for
the health of the bitcoin network.

"serverurl" : This is the IP(s) to try to bind ckpool uniquely to, otherwise it
will attempt to bind to all interfaces in port 3333 by default in pool mode
and 3334 in proxy mode. Multiple entries can be specified as an array by
either IP or resolvable domain name but the executable must be able to bind to
all of them and ports up to 1024 usually require privileged access.

"redirecturl" : This is an array of URLs that ckpool will redirect active
miners to in redirector mode. They must be valid resolvable URLs+ports.

"mindiff" : Minimum diff that vardiff will allow miners to drop to. Default 1

"startdiff" : Starting diff that new clients are given. Default 42

"maxdiff" : Optional maximum diff that vardiff will clamp to where zero is no
maximum.

"logdir" : Which directory to store pool and client logs. Default "logs"

"maxclients" : Optional upper limit on the number of clients ckpool will
accept before rejecting further clients.