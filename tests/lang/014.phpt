--TEST--
Testing eval function inside user-defined function
--FILE--
<?php

function F($a)
{
    eval($a);
}
function fn2126523119()
{
    error_reporting(0);
    F("echo \"Hello\";");
}
fn2126523119();
--EXPECT--
Hello
