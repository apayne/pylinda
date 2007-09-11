(use-modules (pylinda linda))

(define (ping-pong)
    (begin (out uts (list 'ping))
           (in uts (list 'pong))
    )
)

(ping-pong)

