--TEST--
Closure 021: Throwing exception inside lambda
--FILE--
<?php

function fn146204378()
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
fn146204378();
--EXPECT--
string(5) "test!"
