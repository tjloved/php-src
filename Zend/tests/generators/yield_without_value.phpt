--TEST--
yield can be used without a value
--FILE--
<?php

function recv()
{
    while (true) {
        var_dump(yield);
    }
}
function fn1525083416()
{
    $receiver = recv();
    var_dump($receiver->current());
    $receiver->send(1);
    var_dump($receiver->current());
    $receiver->send(2);
    var_dump($receiver->current());
    $receiver->send(3);
}
fn1525083416();
--EXPECT--
NULL
int(1)
NULL
int(2)
NULL
int(3)
