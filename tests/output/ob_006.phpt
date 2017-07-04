--TEST--
output buffering - ob_end_flush
--FILE--
<?php

function fn1324350712()
{
    ob_start();
    echo "foo\n";
    ob_end_flush();
    var_dump(ob_get_level());
}
fn1324350712();
--EXPECT--
foo
int(0)
