--TEST--
output buffering - ob_end_clean
--FILE--
<?php

function fn661862764()
{
    ob_start();
    echo "foo\n";
    ob_start();
    echo "bar\n";
    ob_end_clean();
    echo "baz\n";
}
fn661862764();
--EXPECT--
foo
baz
