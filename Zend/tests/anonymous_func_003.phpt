--TEST--
Using throw $var with anonymous function return
--FILE--
<?php

function fn1171300775()
{
    try {
        $a = create_function('', 'return new Exception("test");');
        throw $a();
    } catch (Exception $e) {
        var_dump($e->getMessage() == 'test');
    }
}
fn1171300775();
--EXPECT--
bool(true)
