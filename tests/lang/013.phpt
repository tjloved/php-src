--TEST--
Testing eval function
--FILE--
<?php

function fn965323617()
{
    error_reporting(0);
    $a = "echo \"Hello\";";
    eval($a);
}
fn965323617();
--EXPECT--
Hello
