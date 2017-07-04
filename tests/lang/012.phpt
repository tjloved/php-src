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
function fn93721803()
{
    $i = 0;
    while ($i < 2) {
        echo F();
        $i++;
    }
}
fn93721803();
--EXPECT--
HelloHello
