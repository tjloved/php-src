--TEST--
Testing user-defined function in included file
--FILE--
<?php

function fn721290278()
{
    include "016.inc";
    MyFunc("Hello");
}
fn721290278();
--EXPECT--
Hello
