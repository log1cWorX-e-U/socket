#pragma once

/// \file socket.h
/// \brief TCP-/UDP-Sockets: Erzeugen, Verbinden, Senden, Empfangen, Schliessen.

#include <string/string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <stddef.h>

/// \brief Ungueltiges Handle bzw. Fehlerwert von socket_create.
#define SOCKET_INVALID_SOCKET -1

/// \brief Allgemeiner Fehlerwert der Socket-Syscalls (z.B. setsockopt, bind, listen).
#define SOCKET_ERROR -1

/// \brief Rueckgabewert bei Zeitueberschreitung einer Empfangsoperation.
#define SOCKET_TIMEOUT -1

/// \brief Handle (Dateideskriptor) eines Sockets.
typedef int socket_handle_t;

/// \brief Prueft per "ping -c 1", ob die Adresse erreichbar ist.
/// \param ip_address Zieladresse (IPv4 oder Hostname).
/// \return true, wenn der Ping-Befehl mit Status 0 endet; sonst false.
/// \note Ruft system() auf und blockiert bis Antwort oder Fehlschlag des Ping.
bool socket_ping(const char* ip_address);

/// \brief Erzeugt einen Socket und setzt dessen Empfangs-Timeout.
/// \param receive_timeout_s Timeout fuer Empfangsoperationen in SEKUNDEN (0 = kein Timeout).
/// \param tcp true fuer TCP (SOCK_STREAM), false fuer UDP (SOCK_DGRAM).
/// \return Socket-Handle bei Erfolg; SOCKET_INVALID_SOCKET, wenn Erzeugen oder setsockopt fehlschlaegt.
/// \note Bei Fehler nach dem Erzeugen wird der Socket bereits wieder geschlossen.
socket_handle_t socket_create(time_t receive_timeout_s, bool tcp);

/// \brief Ermittelt die eigene IP-Adresse ueber die Route zu 8.8.8.8.
/// \param ip Zielpuffer fuer die nullterminierte IP-Adresse.
/// \param ip_size Groesse des Zielpuffers in Bytes.
/// \return true, wenn eine IP ermittelt und in ip abgelegt wurde; sonst false.
/// \note Es werden keine echten Pakete gesendet; die Quell-IP wird per connect/getsockname bestimmt.
bool socket_get_own_ip(char* ip, size_t ip_size);

/// \brief Bindet den Socket an INADDR_ANY:port und beginnt zu lauschen.
/// \param socket Socket-Handle.
/// \param port Portnummer.
/// \return true bei Erfolg (bind und listen); sonst false.
/// \note Der Listen-Backlog ist auf 1 festgelegt.
bool socket_bind_and_listen(socket_handle_t socket, int16_t port);

/// \brief Nimmt eine eingehende Verbindung an (blockierend).
/// \param socket lauschender Socket-Handle.
/// \param receive_timeout_us wird zurzeit NICHT ausgewertet.
/// \return Handle der Client-Verbindung; SOCKET_INVALID_SOCKET bei Fehler oder Timeout.
/// \note Das Warten richtet sich nach dem in socket_create gesetzten Empfangs-Timeout.
socket_handle_t socket_accept_incomming_connection(socket_handle_t socket, time_t receive_timeout_us);

/// \brief Verbindet den Socket mit ip:port (blockierend).
/// \param socket Socket-Handle.
/// \param ip Zieladresse als IPv4-Text.
/// \param port Zielport.
/// \return true bei Erfolg; false, wenn connect fehlschlaegt (z.B. Timeout oder Abweisung).
/// \note Ignoriert bei Erfolg nachfolgend SIGPIPE, damit ein Schliessen durch die Gegenseite nicht abbricht.
bool socket_connect(socket_handle_t socket, const char* ip, unsigned short port);

/// \brief Sendet size_bytes Bytes aus data.
/// \param socket Socket-Handle.
/// \param data Quelldaten.
/// \param size_bytes Anzahl der zu sendenden Bytes.
/// \return true nur, wenn GENAU size_bytes gesendet wurden; sonst false.
/// \note Eine Teilsendung gilt als Fehlschlag; die Restbytes werden nicht nachgesendet.
bool socket_send(socket_handle_t socket, const void* data, size_t size_bytes);

/// \brief Empfaengt bis zu buffer_size Bytes (blockierend bis Timeout).
/// \param socket Socket-Handle.
/// \param data Zielpuffer.
/// \param buffer_size Groesse des Zielpuffers in Bytes.
/// \return Anzahl empfangener Bytes; 0, wenn die Verbindung geschlossen wurde; -1 bei Fehler oder Timeout.
/// \note Der Puffer wird NICHT automatisch nullterminiert.
ssize_t socket_receive(socket_handle_t socket, char* data, size_t buffer_size);

/// \brief Sendet ein UDP-Broadcast und wartet auf die Antwort eines fremden Senders.
/// \param broadcast_ip Broadcast-Adresse (z.B. 255.255.255.255).
/// \param port Zielport.
/// \param data zu sendende Nutzdaten.
/// \param size_bytes Anzahl der Nutzdaten-Bytes.
/// \param ignore_ip eigene IP, die als Antwort ignoriert wird; NULL = nichts ignorieren.
/// \param sender_ip Zielpuffer fuer die IP des antwortenden Senders.
/// \param sender_ip_size Groesse des sender_ip-Puffers in Bytes.
/// \return true, wenn eine (nicht ignorierte) Antwort empfangen und sender_ip gefuellt wurde; sonst false.
/// \note Blockiert in 3-Sekunden-Schritten (select); die empfangenen Nutzdaten werden verworfen.
bool socket_udp_broadcast(const char* broadcast_ip, unsigned short port, const void* data, size_t size_bytes, const char* ignore_ip, char* sender_ip, size_t sender_ip_size);

/// \brief Schliesst den Socket und setzt das Handle auf SOCKET_INVALID_SOCKET.
/// \param socket Zeiger auf den Socket-Handle.
/// \note Ein bereits ungueltiges Handle ist kein Fehler.
void socket_close(socket_handle_t* socket);
