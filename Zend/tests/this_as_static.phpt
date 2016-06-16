--TEST--
$this as static variable
--FILE--
<?php

function foo()
{
    static $this;
    var_dump($this);
}
function fn1322453723()
{
    foo();
}
fn1322453723();
--EXPECTF--
Fatal error: Cannot use $this as static variable in %sthis_as_static.php on line %d
