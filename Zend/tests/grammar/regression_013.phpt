--TEST--
Testing for regression with encapsed variables in class declaration context
--FILE--
<?php

class A
{
    function foo()
    {
        "{${$a}}";
    }
    function list()
    {
    }
}
function fn1681486782()
{
    echo "Done", PHP_EOL;
}
fn1681486782();
--EXPECTF--

Done
