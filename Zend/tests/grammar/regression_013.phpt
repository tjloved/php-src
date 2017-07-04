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
function fn730840503()
{
    echo "Done", PHP_EOL;
}
fn730840503();
--EXPECTF--

Done
