--TEST--
Closure 021: Throwing exception inside lambda
--FILE--
<?php

function fn1145580708()
{
    $foo = function () {
        try {
            throw new Exception('test!');
        } catch (Exception $e) {
            throw $e;
        }
    };
    try {
        $foo();
    } catch (Exception $e) {
        var_dump($e->getMessage());
    }
}
fn1145580708();
--EXPECT--
string(5) "test!"
