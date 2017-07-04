--TEST--
$this as static variable
--FILE--
<?php

function foo()
{
    static $this;
    var_dump($this);
}
function fn104139641()
{
    foo();
}
fn104139641();
--EXPECTF--
Fatal error: Cannot use $this as static variable in %sthis_as_static.php on line %d
