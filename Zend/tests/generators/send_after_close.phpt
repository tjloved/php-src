--TEST--
Calls to send() after close should do nothing
--FILE--
<?php

function gen()
{
    var_dump(yield);
}
function fn584136484()
{
    $gen = gen();
    $gen->send('foo');
    $gen->send('bar');
}
fn584136484();
--EXPECT--
string(3) "foo"
