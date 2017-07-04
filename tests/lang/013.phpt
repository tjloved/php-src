--TEST--
Testing eval function
--FILE--
<?php

function fn115826462()
{
    error_reporting(0);
    $a = "echo \"Hello\";";
    eval($a);
}
fn115826462();
--EXPECT--
Hello
