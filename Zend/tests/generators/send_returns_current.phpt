--TEST--
$generator->send() returns the yielded value
--FILE--
<?php

function reverseEchoGenerator()
{
    $data = yield;
    while (true) {
        $data = (yield strrev($data));
    }
}
function fn23094023()
{
    $gen = reverseEchoGenerator();
    var_dump($gen->send('foo'));
    var_dump($gen->send('bar'));
}
fn23094023();
--EXPECT--
string(3) "oof"
string(3) "rab"
