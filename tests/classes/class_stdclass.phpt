--TEST--
Instantiate stdClass
--FILE--
<?php

function fn1326669054()
{
    $obj = new stdClass();
    echo get_class($obj) . "\n";
    echo "Done\n";
}
fn1326669054();
--EXPECTF--
stdClass
Done
