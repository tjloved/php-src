--TEST--
Stack is cleaned up properly when an exception is thrown during a function call
--FILE--
<?php

function throwException()
{
    throw new Exception('test');
}
function gen()
{
    (yield 'foo');
    strlen("foo", "bar", throwException());
    (yield 'bar');
}
function fn545682846()
{
    $gen = gen();
    var_dump($gen->current());
    try {
        $gen->next();
    } catch (Exception $e) {
        echo 'Caught exception with message "', $e->getMessage(), '"', "\n";
    }
    var_dump($gen->current());
}
fn545682846();
--EXPECT--
string(3) "foo"
Caught exception with message "test"
NULL
