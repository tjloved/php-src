--TEST--
Testing stack after early function return
--FILE--
<?php

function F()
{
    if (1) {
        return "Hello";
    }
}
function fn815816202()
{
    $i = 0;
    while ($i < 2) {
        echo F();
        $i++;
    }
}
fn815816202();
--EXPECT--
HelloHello
