--TEST--
Indirect method call and cloning
--FILE--
<?php

class bar
{
    public $z;
    public function __construct()
    {
        $this->z = new stdclass();
    }
    public function getZ()
    {
        return $this->z;
    }
}
function fn1528002845()
{
    var_dump(clone (new bar())->z);
    var_dump(clone (new bar())->getZ());
}
fn1528002845();
--EXPECTF--
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
