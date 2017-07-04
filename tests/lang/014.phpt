--TEST--
Testing eval function inside user-defined function
--FILE--
<?php

function F($a)
{
    eval($a);
}
function fn1401152177()
{
    error_reporting(0);
    F("echo \"Hello\";");
}
fn1401152177();
--EXPECT--
Hello
