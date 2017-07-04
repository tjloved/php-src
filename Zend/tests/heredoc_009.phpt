--TEST--
Torture the T_END_HEREDOC rules (heredoc)
--FILE--
<?php

function fn535184663()
{
    require_once 'nowdoc.inc';
    print <<<ENDOFHEREDOC
ENDOFHEREDOC    ;
    ENDOFHEREDOC;
ENDOFHEREDOC    
    ENDOFHEREDOC
{$ENDOFHEREDOC};

ENDOFHEREDOC;
    $x = <<<ENDOFHEREDOC
ENDOFHEREDOC    ;
    ENDOFHEREDOC;
ENDOFHEREDOC    
    ENDOFHEREDOC
{$ENDOFHEREDOC};

ENDOFHEREDOC;
    print "{$x}";
}
fn535184663();
--EXPECTF--
Notice: Undefined variable: ENDOFHEREDOC in %s on line %d
ENDOFHEREDOC    ;
    ENDOFHEREDOC;
ENDOFHEREDOC    
    ENDOFHEREDOC
;

Notice: Undefined variable: ENDOFHEREDOC in %s on line %d
ENDOFHEREDOC    ;
    ENDOFHEREDOC;
ENDOFHEREDOC    
    ENDOFHEREDOC
;
