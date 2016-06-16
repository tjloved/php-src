--TEST--
Testing __debugInfo() magic method with bad returns ONE
--FILE--
<?php

class C
{
    public $val;
    public function __debugInfo()
    {
        return $this->val;
    }
    public function __construct($val)
    {
        $this->val = $val;
    }
}
function fn1135227836()
{
    $c = new C(1);
    var_dump($c);
}
fn1135227836();
--EXPECTF--
Fatal error: __debuginfo() must return an array in %s%eZend%etests%edebug_info-error-1.php on line %d
