#!/bin/bash

# user modifiable by environment
SERVER_EXECUTABLE=${SERVER_EXECUTABLE:-../../server/server}
SERVER=${SERVER:-http://localhost:8080}

# mere existence of these changes behaviour
#KEEP_ALIVE -- Do not kill server at end
#DEBUG      -- Prints debug information


echo "Server executable: $SERVER_EXECUTABLE"
echo "Server address: $SERVER"


# code
if [ ! "$DEBUG" ]; then
  $SERVER_EXECUTABLE 2>/dev/null >/dev/null &
  sleep 1
  SERVER_PID=$!
else
  $SERVER_EXECUTABLE &
  sleep 1
  SERVER_PID=$!
fi
echo "Server PID: $SERVER_PID"

nfail=0

function atexit(){
  echo "$nfail failures."  1>&2
  if [ ! "$KEEP_ALIVE" ]; then
    kill $SERVER_PID
  fi
}

function fail(){
  echo "Fail '$*'" 1>&2
  nfail=$(( $nfail +1 ))
  if [ "$1" == "fatal" ]; then
    exit $nfail
  fi
}

function fail_if(){
  $*
  local r=$?
  if [ "$r" == 0 ]; then
    fail "$r: $*"
  fi
}

function fail_if_not(){
  $*
  local r=$?
  if [ "$r" != 0 ]; then
    fail "$r: $*"
  fi
}

function fail_if_not_equal(){
  ret=$( $1 )
  if [ "$ret" != "$2" ]; then
    fail "\$( $1 ) == $2, was $ret"
  fi
}

function fail_if_not_matches(){
  ret=$( $1 )
  if ! echo "$ret" | grep "$2" >/dev/null; then
    fail "\$( $1 | grep '$2', was $ret"
  fi
}

function urlencode(){
  python -c "import urllib; a='''$*'''.split('='); b='='.join(a[1:]); a=a[0]; print '%s=%s'%(urllib.quote(a),urllib.quote(b)),"
}

function GET(){
  curl -s -f $1
  return $?
}

function POST(){
  local CURL="curl -f -s $1 -d"
  shift 1
  CURL="$CURL $( urlencode $1 )"
  shift 1
  if [ "$*" ]; then
    for a in "$*"; do
      CURL="$CURL&$( urlencode $a )"
    done
  fi
  echo $CURL 1>&2
  $CURL
  return $?
}

function POST_FILE(){
  curl $1 -s -f -d $2=@$3 
  return $?
}

trap atexit EXIT
if [ ! "$DEBUG" ]; then
  exec 1>/dev/null
fi

# empty state
fail_if_not       "GET $SERVER/"
fail_if_not_equal "GET $SERVER/node/"  '{}'
fail_if_not_equal "GET $SERVER/connections/" '[]'
fail_if           "GET $SERVER/upload/"

# load start, lua_action
fail_if_not_matches "POST $SERVER/node/ create_node=start" 'node_\d*'
n_start=$ret
fail_if_not_matches "POST $SERVER/node/ create_node=lua_action" 'node_\d*'
n_lua_action=$ret
fail_if_not_equal "POST $SERVER/node/$n_start connect_to=$n_lua_action" 'OK'
fail_if_not_matches "GET $SERVER/node/"  '.*'$n_start'.*'$n_lua_action'.*'
fail_if_not_matches "GET $SERVER/connections/"  '.*from.*'$n_start'.*to.*'$n_lua_action'.*'
fail_if_not_matches "GET $SERVER/connections/"  '.*"guard":"".*'

fail_if_not_equal "POST $SERVER/node/$n_lua_action attr=exec value=print(\"Hola\")" 'OK'
fail_if           "POST $SERVER/node/$n_lua_action attr=does_not_exist value='print \"Error if can add\"'" 'OK'

fail_if_not_matches "GET $SERVER/node/$n_lua_action" '.*Hola.*'

fail_if_not_equal "POST $SERVER/node/$n_start notify=1" 'OK'


exit $nfail
