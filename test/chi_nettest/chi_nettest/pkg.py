"""
This module provides classes and methods to launch the HermesRun service.
chimaera_codegen is ....
"""

from jarvis_cd.basic.pkg import Service, Color
from jarvis_util import *


class ChiNettest(Service):
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
                'name': 'rpc_cpus',
                'msg': 'the mapping of rpc threads to cpus',
                'type': list,
                'default': None,
                'class': 'communication',
                'rank': 1,
                'args': [
                    {
                        'name': 'cpu_id',
                        'msg': 'An integer representing CPU ID',
                        'type': int,
                    }
                ],
            },
        ]

    def get_hostfile(self):
        self.hostfile = self.jarvis.hostfile
        if self.config['num_nodes'] > 0 and self.hostfile.path is not None:
            self.hostfile = Hostfile(hostfile=self.hostfile_path)

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
        if self.config['num_nodes'] > 0 and self.hostfile.path is not None:
            self.hostfile = self.hostfile.subset(self.config['num_nodes'])
            self.hostfile.save(self.hostfile_path)

        # Begin making chimaera_run config
        chimaera_server = {
        }
        
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
        chimaera_server['rpc'] = {
            'host_file': hostfile_path,
            'protocol': protocol,
            'domain': domain,
            'port': self.config['port'],
        }
        if self.config['rpc_cpus'] is not None:
            chimaera_server['rpc']['cpus'] = self.config['rpc_cpus']
        if self.hostfile.path is None:
            chimaera_server['rpc']['host_names'] = self.hostfile.hosts

        # Add some initial modules to the registry
        chimaera_server['module_registry'] = self.config['modules']

        # Save Chimaera configuration
        chimaera_server_yaml = f'{self.shared_dir}/chimaera_server.yaml'
        YamlFile(chimaera_server_yaml).save(chimaera_server)
        self.env['CHIMAERA_CONF'] = chimaera_server_yaml

    def start(self):
        """
        Launch an application. E.g., OrangeFS will launch the servers, clients,
        and metadata services on all necessary pkgs.

        :return: None
        """
        self.log(self.env['CHIMAERA_CONF'])
        self.get_hostfile()
        self.daemon_pkg = Exec('chimaera_start_runtime',
                                PsshExecInfo(hostfile=self.hostfile,
                                             env=self.mod_env,
                                             exec_async=True,
                                             do_dbg=self.config['do_dbg'],
                                             dbg_port=self.config['dbg_port'],
                                             hide_output=self.config['hide_output'],
                                             pipe_stdout=self.config['stdout'],
                                             pipe_stderr=self.config['stderr']))
        time.sleep(self.config['sleep'])
        self.log('Done sleeping')

    def stop(self):
        """
        Stop a running application. E.g., OrangeFS will terminate the servers,
        clients, and metadata services.

        :return: None
        """
        self.log('Stopping chimaera_run')
        self.get_hostfile()
        Exec('chimaera_stop_runtime',
             LocalExecInfo(hostfile=self.hostfile,
                           env=self.env,
                           exec_async=False,
                           # do_dbg=self.config['do_dbg'],
                           # dbg_port=self.config['dbg_port'] + 2,
                           hide_output=self.config['hide_output']))
        self.log('Client Exited?')
        if self.daemon_pkg is not None:
            self.daemon_pkg.wait()
        self.log('Daemon Exited?')

    def kill(self):
        self.get_hostfile()
        Kill('.*chimaera.*',
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
