--TEST--
Try catch finally (re-throw exception in catch block)
--FILE--
<?php

function foo($a)
{
    try {
        throw new Exception("ex");
    } catch (Exception $e) {
        var_dump($a);
        throw $e;
    } finally {
        var_dump("finally");
        return "return";
    }
    return 1;
}
function fn1907841131()
{
    try {
        var_dump(foo("para"));
    } catch (Exception $e) {
        "caught exception" . PHP_EOL;
        var_dump($e->getMessage());
    }
}
fn1907841131();
--EXPECT--
string(4) "para"
string(7) "finally"
string(6) "return"
