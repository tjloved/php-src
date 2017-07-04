--TEST--
ZE2 The inherited destructor is called
--FILE--
<?php

class base
{
    function __construct()
    {
        echo __METHOD__ . "\n";
    }
    function __destruct()
    {
        echo __METHOD__ . "\n";
    }
}
class derived extends base
{
}
function fn1951188814()
{
    $obj = new derived();
    unset($obj);
    echo 'Done';
}
fn1951188814();
--EXPECT--
base::__construct
base::__destruct
Done
