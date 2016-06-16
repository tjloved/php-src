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
function fn108554514()
{
    $gen = reverseEchoGenerator();
    var_dump($gen->send('foo'));
    var_dump($gen->send('bar'));
}
fn108554514();
--EXPECT--
string(3) "oof"
string(3) "rab"
