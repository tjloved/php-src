--TEST--
Testing __debugInfo() magic method with bad returns ONE (float)
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
function fn1432710112()
{
    $c = new C(1.0);
    var_dump($c);
}
fn1432710112();
--EXPECTF--
Fatal error: __debuginfo() must return an array in %s%eZend%etests%edebug_info-error-1.0.php on line %d
