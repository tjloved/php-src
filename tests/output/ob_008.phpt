--TEST--
output buffering - ob_get_contents
--FILE--
<?php

function fn1166311448()
{
    ob_start();
    echo "foo\n";
    echo ob_get_contents();
}
fn1166311448();
--EXPECT--
foo
foo
