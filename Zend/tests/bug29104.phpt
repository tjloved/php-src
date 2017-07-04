--TEST--
Bug #29104 (Function declaration in method doesn't work)
--FILE--
<?php

class A
{
    function g()
    {
        echo "function g - begin\n";
        function f()
        {
            echo "function f\n";
        }
        echo "function g - end\n";
    }
}
function fn951319562()
{
    $a = new A();
    $a->g();
    f();
}
fn951319562();
--EXPECT--
function g - begin
function g - end
function f
