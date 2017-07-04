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
function fn2058946032()
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
fn2058946032();
--EXPECT--
string(3) "foo"
Caught exception with message "test"
NULL
