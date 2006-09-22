parser linda:
    ignore:        '\\s+'
    token END:     '$'
    token INT:     '-?[1-9][0-9]*'
    token FLOAT:   '-?[1-9][0-9]*[.][0-9]+'
    token STRING:  '["][a-zA-Z.\';:#~!£$%^&*()]+["]'
    token TYPE:    'int|float|str|TS'
    token TSID:    'S[0-9a-zA-Z]+|UTS|uts'
    token LIST:    'list'
    token INSPECT: 'inspect'
    token INP:     'inp'
    token RDP:     'rdp'
    token IN:      'in'
    token RD:      'rd'
    token OUT:     'out'
    token VAR:     '[a-zA-Z]+'
    token PLUS:    '[+]'
    token SUB:     '[-]'
    token MUL:     '[*]'
    token DIV:     '[/]'
    token COMMA:   '[,]'
    token OPENB:   '[(]'
    token CLOSEB:  '[)]'
    token EQUAL:   '='
    token USCORE:  '_'

    rule goal: command END {{ return command }}

    rule command: list      {{ return list }}
                | inspect   {{ return inspect }}
                | 'route'   {{ return ("route", ) }}
                | out       {{ return out }}
                | expr      {{ return ("expr", expr) }}

    rule name: TSID {{ return ("tsid", TSID) }}
             | VAR  {{ return ("var", str(VAR)) }}

    rule list: LIST {{ return ("list", ) }}

    rule inspect: INSPECT name  {{ return ("inspect", name) }}

    rule _in:  IN  expr {{ v1, v2 = expr, None }} ( expr {{ v2 = expr }} )?  {{ return ("in", v1, v2) }}
    rule rd:   RD  expr {{ v1, v2 = expr, None }} ( expr {{ v2 = expr }} )?  {{ return ("rd", v1, v2) }}
    rule _inp: INP expr {{ v1, v2 = expr, None }} ( expr {{ v2 = expr }} )?  {{ return ("inp", v1, v2) }}
    rule rdp:  RDP expr {{ v1, v2 = expr, None }} ( expr {{ v2 = expr }} )?  {{ return ("rdp", v1, v2) }}
    rule out:  OUT expr {{ v1, v2 = expr, None }} ( expr {{ v2 = expr }} )?  {{ return ("out", v1, v2) }}

    rule expr: bracket {{ v = bracket }}
               ( EQUAL expr {{ v = ("assign", v, expr) }} )?
               {{ return v }}

    rule bracket: OPENB linda CLOSEB {{ return linda }}
             | linda {{ return linda }}

    rule linda: _in       {{ return _in }}
              | rd        {{ return rd }}
              | _inp      {{ return _inp }}
              | rdp       {{ return rdp }}
              | tuple     {{ return tuple }}

    rule tuple: sum       {{ v = sum }}
             ( ( COMMA    {{ v = ("tuple", (v, )) }} )
             ( sum        {{ v = ("tuple", v[1] + (sum, )) }}
               (COMMA sum {{ v = ("tuple", v[1] + (sum, )) }} )*
             )? )?        {{ return v }}

    rule sum : factor    {{ v = factor }}
             ( PLUS expr {{ v = ("+", v, expr) }}
             | SUB expr  {{ v = ("-", v, expr) }}
             )*          {{ return v }}

    rule factor: funccall   {{ v = funccall }}
               ( MUL factor {{ v = ("*", v, factor) }}
               | DIV factor {{ v = ("/", v, factor) }}
               )*           {{ return v }}

    rule funccall: single_expr {{ v = single_expr }}
                 ( OPENB       {{ args = ("tuple", ()) }}
                   ( tuple     {{ args = tuple }} )?
                   CLOSEB      {{ v = ("funccall", v, args) }}
                 )?            {{ return v }}

    rule single_expr:   INT       {{ return ("val", int(INT)) }}
                      | FLOAT     {{ return ("val", float(FLOAT)) }}
                      | STRING    {{ return ("val", eval(STRING)) }}
                      | name      {{ return name }}
                      | USCORE    {{ return ("skip", ) }}
