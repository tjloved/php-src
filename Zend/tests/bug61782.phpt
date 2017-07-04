--TEST--
Bug #61782 (__clone/__destruct do not match other methods when checking access controls)
--FILE--
<?php

abstract class BaseClass
{
    protected abstract function __clone();
}
class MommasBoy extends BaseClass
{
    protected function __clone()
    {
        echo __METHOD__, "\n";
    }
}
class LatchkeyKid extends BaseClass
{
    public function __construct()
    {
        echo 'In ', __CLASS__, ":\n";
        $kid = new MommasBoy();
        $kid = clone $kid;
    }
    public function __clone()
    {
    }
}
function fn1707568408()
{
    $obj = new LatchkeyKid();
    echo "DONE\n";
}
fn1707568408();
--EXPECT--
In LatchkeyKid:
MommasBoy::__clone
DONE
