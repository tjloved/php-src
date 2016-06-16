--TEST--
Generators can throw exceptions
--FILE--
<?php

function gen()
{
    (yield 'foo');
    throw new Exception('test');
    (yield 'bar');
}
function fn555789337()
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
fn555789337();
--EXPECT--
string(3) "foo"
Caught exception with message "test"
NULL
