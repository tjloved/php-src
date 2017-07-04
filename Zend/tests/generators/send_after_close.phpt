--TEST--
Calls to send() after close should do nothing
--FILE--
<?php

function gen()
{
    var_dump(yield);
}
function fn1155481742()
{
    $gen = gen();
    $gen->send('foo');
    $gen->send('bar');
}
fn1155481742();
--EXPECT--
string(3) "foo"
