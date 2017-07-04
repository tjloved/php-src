--TEST--
Dtor may throw exception furing FE_FETCH assignment
--FILE--
<?php

function fn1371440730()
{
    $v = new class
    {
        function __destruct()
        {
            throw new Exception("foo");
        }
    };
    try {
        foreach ([1, 2] as $v) {
            var_dump($v);
        }
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
}
fn1371440730();
--EXPECT--
foo
