--TEST--
"yield" can occur during a function call
--FILE--
<?php

function gen()
{
    var_dump(str_repeat("x", yield));
}
function fn1181341419()
{
    $gen = gen();
    $gen->send(10);
}
fn1181341419();
--EXPECT--
string(10) "xxxxxxxxxx"
