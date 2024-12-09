"""
This module provides classes and methods to launch the HermesRun service.
chimaera_codegen is ....
"""

from jarvis_cd.basic.pkg import Service, Color
from jarvis_util import *


class ChiNetBench(Service):
    """
    This class provides methods to launch the HermesRun service.
    """
    def _init(self):
        """
        Initialize paths
        """
        self.daemon_pkg = None
        self.hostfile_path = f'{self.shared_dir}/hostfile'
        pass

    def _configure_menu(self):
        """
        Create a CLI menu for the configurator method.
        For thorough documentation of these parameters, view:
        https://github.com/scs-lab/jarvis-util/wiki/3.-Argument-Parsing

        :return: List(dict)
        """
        return [
            {
                'name': 'test',
                'msg': 'The test case to run',
                'type': str,
                'default': 'ping',
                'class': 'communication',
                'rank': 1,
            },
            {
                'name': 'port',
                'msg': 'The port to listen for data on',
                'type': int,
                'default': 8080,
                'class': 'communication',
                'rank': 1,
            },
            {
                'name': 'provider',
                'msg': 'The libfabric provider type to use (e.g., sockets)',
                'type': str,
                'default': None,
                'class': 'communication',
                'rank': 1,
            },
            {
                'name': 'domain',
                'msg': 'The libfabric domain to use (e.g., lo)',
                'type': str,
                'default': None,
                'class': 'communication',
                'rank': 1,
            },
            {
                'name': 'rpc_threads',
                'msg': 'the mapping of rpc threads to cpus',
                'type': int,
                'default': 4,
                'class': 'communication',
                'rank': 1,
            },
            {
                'name': 'io_size',
                'msg': 'I/O size for the test',
                'type': str,
                'default': '1m',
                'class': 'communication',
                'rank': 1,
            },
            {
                'name': 'md_size',
                'msg': 'Metadata size for the test',
                'type': str,
                'default': '4k',
                'class': 'communication',
                'rank': 1,
            },
            {
                'name': 'rep',
                'msg': 'Number of repititions',
                'type': int,
                'default': 1,
                'class': 'communication',
                'rank': 1,
            },
        ]

    def get_hostfile(self):
        self.hostfile = self.jarvis.hostfile
        
    def _configure(self, **kwargs):
        """
        Converts the Jarvis configuration to application-specific configuration.
        E.g., OrangeFS produces an orangefs.xml file.

        :param config: The human-readable jarvis YAML configuration for the
        application.
        :return: None
        """
        rg = self.jarvis.resource_graph

        # Create hostfile
        self.hostfile = self.jarvis.hostfile
        
        # Get network Info
        if len(self.hostfile) > 1:
            net_info = rg.find_net_info(self.hostfile, strip_ips=True, shared=True)
        else:
            # net_info = rg.find_net_info(self.hostfile, strip_ips=True)
            net_info = rg.find_net_info(self.hostfile)
        provider = self.config['provider']
        if provider is None:
            opts = net_info['provider'].unique().list()
            order = ['sockets', 'tcp', 'udp', 'verbs', 'ib']
            for opt in order:
                if opt in opts:
                    provider = opt
                    break
            if provider is None:
                provider = opts[0]
        self.log(f'Provider: {provider}')
        net_info_save = net_info
        net_info = net_info[lambda r: str(r['provider']) == provider,
                            ['provider', 'domain']]
        if len(net_info) == 0:
            self.log(net_info_save)
            raise Exception(f'Failed to find chimaera_run provider {provider}')
        net_info = net_info.rows[0]
        protocol = net_info['provider']
        domain = net_info['domain']
        hostfile_path = self.hostfile.path
        if self.hostfile.path is None:
            hostfile_path = ''
            domain = ''
        if self.config['domain'] is not None:
            domain = self.config['domain']
        
        self.config['_host_file'] = hostfile_path
        self.config['_protocol'] = protocol
        self.config['_domain'] = domain
        self.config['_port'] = self.config['port']
        self.config['_rpc_threads'] = self.config['rpc_threads']

    def start(self):
        """
        Launch an application. E.g., OrangeFS will launch the servers, clients,
        and metadata services on all necessary pkgs.

        :return: None
        """
        self.get_hostfile()
        # <test> <hostfile> <domain> <protocol> <port> <num_threads>
        #    <io_size> <md_size> <rep> <sleep>
        cmd = [
            'chi_net_bench',
            self.config['test'],
            self.config['_host_file'],
            self.config['_domain'],
            self.config['_protocol'],
            self.config['_port'],
            self.config['_rpc_threads'],
            self.config['io_size'],
            self.config['md_size'],
            self.config['rep'],
            self.config['sleep']
        ]
        cmd = ' '.join([f'\'{str(c)}\'' for c in cmd])
        self.log(cmd, Color.YELLOW)
        self.daemon_pkg = Exec(cmd,
                                PsshExecInfo(hostfile=self.hostfile,
                                             env=self.mod_env,
                                             do_dbg=self.config['do_dbg'],
                                             dbg_port=self.config['dbg_port'],
                                             hide_output=self.config['hide_output'],
                                             pipe_stdout=self.config['stdout'],
                                             pipe_stderr=self.config['stderr']))

    def stop(self):
        """
        Stop a running application. E.g., OrangeFS will terminate the servers,
        clients, and metadata services.

        :return: None
        """
        self.kill()

    def kill(self):
        self.get_hostfile()
        Kill('.*chi_net_bench.*',
             PsshExecInfo(hostfile=self.hostfile,
                          env=self.env))
        self.log('Client Exited?')
        if self.daemon_pkg is not None:
            self.daemon_pkg.wait()
        self.log('Daemon Exited?')

    def clean(self):
        """
        Destroy all data for an application. E.g., OrangeFS will delete all
        metadata and data directories in addition to the orangefs.xml file.

        :return: None
        """
        pass

    def status(self):
        """
        Check whether or not an application is running. E.g., are OrangeFS
        servers running?

        :return: True or false
        """
        return True
