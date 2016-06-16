--TEST--
Try finally (exception in "return" statement)
--FILE--
<?php

class A
{
    public $x = 1;
    public $y = 2;
    function __destruct()
    {
        throw new Exception();
    }
}
function foo()
{
    foreach (new A() as $a) {
        try {
            return $a;
        } catch (Exception $e) {
            echo "exception in foo\n";
        } finally {
            echo "finally\n";
        }
    }
}
function fn58189933()
{
    try {
        foo();
    } catch (Exception $e) {
        echo "exception in main\n";
    }
}
fn58189933();
--EXPECT--
finally
exception in main

