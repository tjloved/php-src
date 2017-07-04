--TEST--
Testing user-defined function in included file
--FILE--
<?php

function fn345619219()
{
    include "016.inc";
    MyFunc("Hello");
}
fn345619219();
--EXPECT--
Hello
