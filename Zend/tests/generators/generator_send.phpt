--TEST--
Values can be sent back to the generator
--FILE--
<?php

function gen()
{
    var_dump((yield "yield foo"));
    var_dump((yield "yield bar"));
}
function fn1441496177()
{
    $gen = gen();
    var_dump($gen->current());
    $gen->send("send bar");
    var_dump($gen->current());
    $gen->send("send foo");
}
fn1441496177();
--EXPECT--
string(9) "yield foo"
string(8) "send bar"
string(9) "yield bar"
string(8) "send foo"
