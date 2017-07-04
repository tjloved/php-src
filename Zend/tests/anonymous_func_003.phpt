--TEST--
Using throw $var with anonymous function return
--FILE--
<?php

function fn1994135588()
{
    try {
        $a = create_function('', 'return new Exception("test");');
        throw $a();
    } catch (Exception $e) {
        var_dump($e->getMessage() == 'test');
    }
}
fn1994135588();
--EXPECTF--
Deprecated: Function create_function() is deprecated in %s on line %d
bool(true)
