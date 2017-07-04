--TEST--
output buffering - ob_get_contents
--FILE--
<?php

function fn1982254333()
{
    ob_start();
    echo "foo\n";
    echo ob_get_contents();
}
fn1982254333();
--EXPECT--
foo
foo
