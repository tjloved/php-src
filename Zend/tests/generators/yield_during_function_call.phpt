--TEST--
"yield" can occur during a function call
--FILE--
<?php

function gen()
{
    var_dump(str_repeat("x", yield));
}
function fn1122892676()
{
    $gen = gen();
    $gen->send(10);
}
fn1122892676();
--EXPECT--
string(10) "xxxxxxxxxx"
