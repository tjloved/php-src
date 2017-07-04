--TEST--
Bug #48428 (crash when exception is thrown while passing function arguments)
--FILE--
<?php

function fn1077374210()
{
    try {
        function x()
        {
            throw new Exception("ERROR");
        }
        x(x());
    } catch (Exception $e) {
        echo $e->getMessage();
    }
}
fn1077374210();
--EXPECT--
ERROR
