--TEST--
output buffering - ob_end_clean
--FILE--
<?php

function fn1186510582()
{
    ob_start();
    echo "foo\n";
    ob_start();
    echo "bar\n";
    ob_end_clean();
    echo "baz\n";
}
fn1186510582();
--EXPECT--
foo
baz
